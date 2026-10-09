# ESP32 Telemetry

Aquisição calibrada, qualidade de sinal, sequência por sensor, buffer limitado e confirmação de entrega serial.

## Executar

Requisitos: ESP32, C++17 e PlatformIO.

```sh
pio test -e native
pio run -e esp32dev
pio run -e esp32dev -t upload
python -m pip install -r cloud/requirements.txt
python cloud/serial_bridge.py /dev/ttyUSB0
```

## Funcionamento

ADC no GPIO 34, serial a 115200 baud. Uptime estendido atravessa o rollover de `millis()`. Identidade de inicialização distingue reinícios. A ponte conserva lotes em SQLite; calibração e pinagem devem corresponder ao equipamento conectado.

## Persistência de resultados

O arquivo de operações está em [vercel-home-telemetry-api.vercel.app](https://vercel-home-telemetry-api.vercel.app/laboratory.html?project=arduino-cloud-telemetry). As migrações Supabase estão no [repositório da API](https://github.com/brunnojob/vercel-home-telemetry-api/tree/main/supabase/migrations).

```sh
python cloud/sync.py enqueue resultado.json --project arduino-cloud-telemetry
python cloud/sync.py sync
```

Defina `BRUNNODEV_ACCESS_TOKEN` com sua sessão. A fila SQLite conserva os relatórios até confirmação do servidor; o mesmo conteúdo não gera registros duplicados. Tokens não são gravados no código.
