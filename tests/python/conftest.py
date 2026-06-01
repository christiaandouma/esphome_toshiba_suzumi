"""
Install minimal ESPHome module stubs into sys.modules before any test file
imports components/toshiba_suzumi/climate.py.

These stubs only need to make the module-level code in climate.py run without
errors (schema construction, class registration). Individual test functions
replace the relevant globals with proper AsyncMock objects using patch.object.
"""
import sys
from unittest.mock import MagicMock


# ── esphome.const ─────────────────────────────────────────────────────────────

class _FakeConst:
    CONF_ID = "id"
    STATE_CLASS_MEASUREMENT = "measurement"
    UNIT_CELSIUS = "°C"
    UNIT_PERCENT = "%"
    UNIT_AMPERE = "A"
    DEVICE_CLASS_TEMPERATURE = "temperature"
    DEVICE_CLASS_CURRENT = "current"
    __version__ = "2026.1.0"


# ── module stubs (MagicMock — only used to survive the import-time code) ──────

sensor_stub = MagicMock()
climate_stub = MagicMock()
uart_stub    = MagicMock()
select_stub  = MagicMock()
cg_stub      = MagicMock()

components_stub = MagicMock()
components_stub.sensor  = sensor_stub
components_stub.climate = climate_stub
components_stub.uart    = uart_stub
components_stub.select  = select_stub

sys.modules.update({
    "esphome":                    MagicMock(),
    "esphome.codegen":            cg_stub,
    "esphome.config_validation":  MagicMock(),
    "esphome.components":         components_stub,
    "esphome.components.sensor":  sensor_stub,
    "esphome.components.climate": climate_stub,
    "esphome.components.uart":    uart_stub,
    "esphome.components.select":  select_stub,
    "esphome.const":              _FakeConst,
    "packaging":                  MagicMock(),
    "packaging.version":          MagicMock(),
})
