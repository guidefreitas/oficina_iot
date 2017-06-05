# Oficina IoT

Códigos da atividade de Internet das coisas utilizando ESP8266

# Hardware 

- ESP8266
- Sensor de temperatura e umidade DHT22
- Tela OLED SSD1306
- Led
- Protoboard e fios

# Bibliotecas necessárias

- Adafruit DHT Sensor - [https://github.com/adafruit/DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)
- ArduinoJson - [https://github.com/bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- Esp8266 Oled SSD1306 - [https://github.com/squix78/esp8266-oled-ssd1306](https://github.com/squix78/esp8266-oled-ssd1306)

# Funcionamento

Este código envia as informações do sensor para o serviço Adafruit.io ([http://adafruit.io](http://adafruit.io)). Com os dados no serviço é possível configurar o aplicativo IFTTT ([https://ifttt.com](https://ifttt.com)) para enviar notificações, armazenar os dados ou realizar diferentes ações.