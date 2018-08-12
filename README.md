# aws-mqtt-websocket-wifimanager-mh-z19
ESP32 arduino MH-z19 CO2 sensor data to AWS IoT using MQTT over Websocket

Platform IDE for Visual Studio Codeでの開発用のArduinoプログラム。

MH-ET Live ESP32 Minikit (ESP-WROOM-32)を用いて CO2センサーである MH-z19 のデータを AWS IoT に MQTT over Websocket で送るプログラム。

WiFiManagerを用いて WiFi接続を行う。

MH-z19 とはシリアル通信を行って、温度と二酸化炭素濃度を取得する。

WeMos D1 Mini 用の OLED display に温度と二酸化炭素濃度を表示すると共に、MQTT で AWS IoTに送信する。

# 設定
aws-mqtt-websocket.cpp 中の次のインクルードファイルはこのリポジトリに含まれていない。
* myAWSus-east2.h

AWS IoTと通信するために以下のマクロを定義する必要がある。

* MY_AWS_ENDPOINT
* MY_AWS_IAM_KEY
* MY_AWS_IAM_SECRET_KEY
* MY_AWS_REGION

# ファイル
各ファイルの主な役割は以下の通り。

* main.cpp
    * メインプログラム
    * [ArduinoJson](https://github.com/bblanchon/ArduinoJson)でJSONの処理をしている。
    * [ArduinoLog](https://github.com/thijse/Arduino-Log/)を使ってデバッグメッセージ出力を制御している。

* wifiManager.cpp
    * [WiFiManager](https://github.com/tzapu/WiFiManager) を使って、WiFi接続をする

* aws-mqtt-websocket.cpp
    * AWS IoT と MQTT over Websocketを使って通信するためのコード
    * [aws-mqtt-websockets](https://github.com/odelot/aws-mqtt-websockets) を利用している

* mh-z19-serial.cpp
    * 二酸化炭素センサー MH-z19 からシリアル通信でCO2濃度と温度を取得するコード
    * ESP32ではハードシリアルを利用する。

* wemos-oled-display.cpp
    * WeMos D1 mini OLED display に表示するためのコード
    * SparkFun もしくは Adafruitの OLED ライブラリを利用

* hardReset.cpp
    * GPIO26 と RST端子を接続しておき、MQTTでのコマンドによりハードリセットするためのコード

* setupDeviceName.cpp
    * デバイス名を設定するための補助コード


# ライブラリ

以下のバージョンのライブラリで動作確認をした。


```
Dependency Graph
|-- <ArduinoJson> 5.13.2
|-- <WebServer> 1.0
|   |-- <FS> 1.0
|   |-- <WiFi> 1.0
|-- <WifiManager> 0.12
|   |-- <DNSServer> 1.1.0
|   |   |-- <WiFi> 1.0
|   |-- <WebServer> 1.0
|   |   |-- <FS> 1.0
|   |   |-- <WiFi> 1.0
|   |-- <ESPmDNS> 1.0
|   |   |-- <WiFi> 1.0
|   |-- <WiFi> 1.0
|-- <Wire> 1.0
|-- <SparkFun Micro OLED Breakout>
|   |-- <SPI> 1.0
|   |-- <Wire> 1.0
|-- <SPI> 1.0
|-- <DNSServer> 1.1.0
|   |-- <WiFi> 1.0
|-- <Paho> 1.0.0
|   |-- <SPI> 1.0
|   |-- <WiFi> 1.0
|-- <PubSubClient> 2.6
|-- <WiFi> 1.0
|-- <WebSockets> 2.1.0
|   |-- <SPI> 1.0
|   |-- <WiFiClientSecure> 1.0
|   |   |-- <WiFi> 1.0
|   |-- <WiFi> 1.0
|-- <ArduinoLog> 1.0.2
|-- <aws-mqtt-websockets>
|   |-- <WebSockets> 2.1.0
|   |   |-- <SPI> 1.0
|   |   |-- <WiFiClientSecure> 1.0
|   |   |   |-- <WiFi> 1.0
|   |   |-- <WiFi> 1.0
|   |-- <aws-sdk-arduino-ESP32>
|   |   |-- <WiFiClientSecure> 1.0
|   |   |   |-- <WiFi> 1.0
|   |   |-- <WiFi> 1.0
```
