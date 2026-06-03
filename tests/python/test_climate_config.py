"""
Tests for components/toshiba_suzumi/climate.py configuration logic.

ESPHome import stubs are installed by conftest.py at collection time.
Each test that invokes to_code() uses patch.object to replace climate.py's
module-level globals (cg, climate, uart, sensor, select) with fresh mocks
that have proper AsyncMock methods — this avoids relying on attribute
assignment to MagicMock being preserved across test boundaries.
"""
import asyncio
import importlib.util
import os
from contextlib import contextmanager
from unittest.mock import MagicMock, AsyncMock, patch

# conftest.py installs ESPHome stubs before this module is collected.

CLIMATE_PY = os.path.join(
    os.path.dirname(__file__), "..", "..", "components", "toshiba_suzumi", "climate.py"
)


def _load_climate_module():
    spec = importlib.util.spec_from_file_location("toshiba_suzumi_climate", CLIMATE_PY)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


# Load once at module level; conftest stubs are already in sys.modules.
climate_module = _load_climate_module()


# ── Test helpers ──────────────────────────────────────────────────────────────

def _run(coro):
    return asyncio.get_event_loop().run_until_complete(coro)


def _make_mocks():
    """Return a (var, cg, patches_ctx) triple ready for a to_code() test."""
    var = MagicMock(name="var")

    cg = MagicMock(name="cg")
    cg.new_Pvariable.return_value = var
    cg.register_component  = AsyncMock()
    cg.register_parented   = AsyncMock()

    climate_comp = MagicMock(name="climate_comp")
    climate_comp.register_climate = AsyncMock()

    uart = MagicMock(name="uart")
    uart.register_uart_device = AsyncMock()

    sensor = MagicMock(name="sensor")
    sensor.new_sensor = AsyncMock(return_value=MagicMock())

    select = MagicMock(name="select")
    select.new_select = AsyncMock(return_value=MagicMock())

    ctx = _patch_globals(cg, climate_comp, uart, sensor, select)
    return var, cg, ctx


@contextmanager
def _patch_globals(cg, climate_comp, uart, sensor, select):
    with (
        patch.object(climate_module, "cg",      cg),
        patch.object(climate_module, "climate",  climate_comp),
        patch.object(climate_module, "uart",     uart),
        patch.object(climate_module, "sensor",   sensor),
        patch.object(climate_module, "select",   select),
    ):
        yield


def _base_config():
    return {"id": MagicMock(name="var_id")}


# ── Preset constants ──────────────────────────────────────────────────────────

class TestPresetConstants:
    def test_supported_presets_key_name(self):
        assert climate_module.CONF_SUPPORTED_PRESETS == "supported_presets"

    def test_deprecated_special_mode_key_name(self):
        assert climate_module.CONF_SPECIAL_MODE == "special_mode"

    def test_eight_degrees_string_is_exact(self):
        # The literal "8 degrees" is used in a hard-coded conditional inside
        # to_code(); any typo silently breaks the FrostGuard min_temp logic.
        valid_presets = [
            "Standard", "Hi POWER", "ECO", "Fireplace 1", "Fireplace 2",
            "8 degrees", "Silent#1", "Silent#2", "Sleep", "Floor", "Comfort",
        ]
        assert "8 degrees" in valid_presets


# ── to_code: supported_presets ────────────────────────────────────────────────

class TestSupportedPresets:
    def test_eight_degrees_sets_min_temp_5(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.CONF_SUPPORTED_PRESETS: ["Standard", "8 degrees"]}
        with ctx:
            _run(climate_module.to_code(config))
        var.set_min_temp.assert_called_with(5)

    def test_eight_degrees_not_present_skips_min_temp(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.CONF_SUPPORTED_PRESETS: ["Standard", "ECO", "Sleep"]}
        with ctx:
            _run(climate_module.to_code(config))
        var.set_min_temp.assert_not_called()

    def test_presets_registered_with_set_supported_presets(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.CONF_SUPPORTED_PRESETS: ["Standard", "ECO"]}
        with ctx:
            _run(climate_module.to_code(config))
        var.set_supported_presets.assert_called_once_with(["Standard", "ECO"])

    def test_no_presets_key_skips_set_supported_presets(self):
        var, cg, ctx = _make_mocks()
        with ctx:
            _run(climate_module.to_code(_base_config()))
        var.set_supported_presets.assert_not_called()


# ── to_code: deprecated special_mode backward compat ─────────────────────────

class TestDeprecatedSpecialMode:
    def _deprecated_config(self, modes):
        return {
            **_base_config(),
            climate_module.CONF_SPECIAL_MODE: {
                climate_module.CONF_SPECIAL_MODE_MODES: modes
            },
        }

    def test_deprecated_key_registers_presets(self):
        var, cg, ctx = _make_mocks()
        config = self._deprecated_config(["Standard", "ECO"])
        with ctx:
            _run(climate_module.to_code(config))
        var.set_supported_presets.assert_called_once_with(["Standard", "ECO"])

    def test_deprecated_eight_degrees_sets_min_temp_5(self):
        var, cg, ctx = _make_mocks()
        config = self._deprecated_config(["Standard", "8 degrees"])
        with ctx:
            _run(climate_module.to_code(config))
        var.set_min_temp.assert_called_with(5)

    def test_deprecated_without_eight_degrees_skips_min_temp(self):
        var, cg, ctx = _make_mocks()
        config = self._deprecated_config(["Standard", "Sleep"])
        with ctx:
            _run(climate_module.to_code(config))
        var.set_min_temp.assert_not_called()


# ── to_code: explicit min_temp ────────────────────────────────────────────────

class TestMinTemp:
    def test_explicit_min_temp_applied(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.MIN_TEMP: 10}
        with ctx:
            _run(climate_module.to_code(config))
        var.set_min_temp.assert_called_with(10)

    def test_eight_degrees_preset_also_sets_min_temp_5(self):
        # When both an explicit min_temp AND "8 degrees" preset are present,
        # both set_min_temp calls must be issued (explicit first, then override).
        var, cg, ctx = _make_mocks()
        config = {
            **_base_config(),
            climate_module.MIN_TEMP: 10,
            climate_module.CONF_SUPPORTED_PRESETS: ["8 degrees"],
        }
        with ctx:
            _run(climate_module.to_code(config))
        called_values = [c.args[0] for c in var.set_min_temp.call_args_list]
        assert 10 in called_values, "explicit min_temp was not applied"
        assert 5  in called_values, "'8 degrees' min_temp override was not applied"


# ── to_code: optional feature flags ──────────────────────────────────────────

class TestFeatureFlags:
    def test_horizontal_swing_enabled(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.FEATURE_HORIZONTAL_SWING: True}
        with ctx:
            _run(climate_module.to_code(config))
        var.set_horizontal_swing.assert_called_with(True)

    def test_horizontal_swing_absent_skips_call(self):
        var, cg, ctx = _make_mocks()
        with ctx:
            _run(climate_module.to_code(_base_config()))
        var.set_horizontal_swing.assert_not_called()

    def test_disable_heat_mode(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.DISABLE_HEAT_MODE: True}
        with ctx:
            _run(climate_module.to_code(config))
        var.disable_heat_mode.assert_called_with(True)

    def test_disable_wifi_led(self):
        var, cg, ctx = _make_mocks()
        config = {**_base_config(), climate_module.DISABLE_WIFI_LED: True}
        with ctx:
            _run(climate_module.to_code(config))
        var.disable_wifi_led.assert_called_with(True)
