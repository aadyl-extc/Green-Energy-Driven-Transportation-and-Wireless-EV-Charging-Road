#include <WiFi.h>
#include <WebServer.h>
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 21
#define RST_PIN 22
#define SWITCH_PIN 5

const char* ssid = "ESP32_AP";
const char* password = "password";

WebServer server(80);

const char* htmlContent = R"=====(
<!DOCTYPE html>
<html>

<head>
    <title>Solar and Wind Charging Station Dashboard</title>
    <!--<script src="https://cdn.jsdelivr.net/npm/chart.js"></script> <!-- Add Chart.js library -->-->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.0/chart.min.js"></script>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #f0f0f0; /* Light gray background */
            margin: 0;
            padding: 0;
        }

        header {
            background-color: rgba(0, 0, 0, 0.8); /* Semi-transparent black background for header */
            color: #fff;
            padding: 20px;
            text-align: center;
        }

        header h1 {
            margin: 0;
            font-size: 48px; /* Increased font size */
            color: #fff; /* White text color */
            background-color: #4CAF50; /* Green background color for the title */
            padding: 10px 0; /* Add padding to the title */
            border-radius: 10px; /* Rounded corners for the title */
        }

        header p {
            margin-top: 10px; /* Add margin to separate the title and header */
            font-size: 24px; /* Font size for the header */
            color: #888; /* Gray color for the header text */
        }

        #gauge-container {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }

        .box-container {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-wrap: wrap;
            padding: 20px;
            margin-bottom: 20px;
        }

        .box {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            width: 300px;
            height: 200px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.4);
            margin: 20px;
            transition: transform 0.2s ease-in-out;
            cursor: pointer;
            text-align: center; /* Center text horizontally */
        }

        .box:hover {
            transform: scale(1.05);
        }

        .gauge-info {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin: 10px;
        }

        .gauge-value {
            font-size: 36px;
            font-weight: bold;
            color: #333;
        }

        .gauge-label {
            font-size: 18px;
            color: #666;
        }

        .solar {
            background-image: linear-gradient(to bottom, #ffd717, #e6ba05);
        }

        .wind {
            background-image: linear-gradient(to bottom, #00e5ff, #00b0ff);
        }

        .box.rfid-status {
            background-color: #fff; /* White background color for the RFID box */
            border-radius: 10px; /* Rounded corners for the RFID box */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.4);
            width: 300px;
            height: 200px;
            margin: 20px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            cursor: pointer;
            text-align: center; /* Center text horizontally */
        }

        .box.rfid-status:hover {
            transform: scale(1.05);
        }

        .container-box {
            background-color: #d0f0c0; /* Light green background color for the container box */
            border-radius: 10px; /* Rounded corners for the container box */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.4);
            width: 940px; /* Adjust the width as needed */
            padding: 20px;
            margin: 20px;
        }

        .container-box.full-width {
            width: 100%; /* Make the container box full width */
        }

        /* Add CSS styles for the chart containers */
        .chart-container {
            width: 1800px;
            height: 500px; /* Adjust the height as needed */
        }
    </style>
</head>

<body>
    <header>
        <h1>Renewable Energy based Electric Vehicle Charging Station</h1>
        <p>DASHBOARD</p>
    </header>
    <div id="gauge-container">
        <!-- First Line - Solar Data -->
        <div class="container-box solar">
            <div class="box-container">
                <div class="box solar">
                    <div class="gauge-info">
                        <div class="gauge-label">Solar Voltage</div>
                        <div class="gauge-value" id="solar-voltage-value">0 V</div>
                    </div>
                </div>
                <div class="box solar">
                    <div class="gauge-info">
                        <div class="gauge-label">Solar Current</div>
                        <div class="gauge-value" id="solar-current-value">0 A</div>
                    </div>
                </div>
                <div class="box solar">
                    <div class="gauge-info">
                        <div class="gauge-label">Solar Watt</div>
                        <div class="gauge-value" id="solar-watt-value">0 W</div>
                    </div>
                </div>
            </div>
        </div>
        <div class="chart-container">
            <canvas id="solarChart"></canvas>
        </div>
        <!-- Second Line - Wind Data -->
        <div class="container-box wind">
            <div class="box-container">
                <div class="box wind">
                    <div class="gauge-info">
                        <div class="gauge-label">Wind Voltage</div>
                        <div class="gauge-value" id="wind-voltage-value">0 V</div>
                    </div>
                </div>
                <div class="box wind">
                    <div class="gauge-info">
                        <div class="gauge-label">Wind Current</div>
                        <div class="gauge-value" id="wind-current-value">0 A</div>
                    </div>
                </div>
                <div class="box wind">
                    <div class="gauge-info">
                        <div class="gauge-label">Wind Watt</div>
                        <div class="gauge-value" id="wind-watt-value">0 W</div>
                    </div>
                </div>
            </div>
        </div>
        <div class="chart-container">
            <canvas id="windChart"></canvas>
        </div>
        <!-- Third Line - RFID Status -->
        <div class="container-box full-width">
            <div class="box-container">
                <div class="box rfid-status">
                    <div class="gauge-info">
                        <div class="gauge-label">RFID Status</div>
                        <div class="gauge-value" id="rfid-status-value">Unauthorized</div>
                    </div>
                </div>
            </div>
        </div>
    </div>
    
    <script>
        // JavaScript code to update the sensor data and RFID status
        var solarChart;
        var windChart;
        
        function updateSensorData() {
            // Make a request to the server to fetch the sensor data
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Update the HTML elements with the received data
                    document.getElementById('solar-voltage-value').innerText = data.solarVoltage + ' V';
                    document.getElementById('solar-current-value').innerText = data.solarCurrent + ' A';
                    document.getElementById('solar-watt-value').innerText = (data.solarVoltage * data.solarCurrent).toFixed(2) + ' W';
                    document.getElementById('wind-voltage-value').innerText = data.windVoltage + ' V';
                    document.getElementById('wind-current-value').innerText = data.windCurrent + ' A';
                    document.getElementById('wind-watt-value').innerText = (data.windVoltage * data.windCurrent).toFixed(2) + ' W';

                    // Update the charts with the new data
                    updateCharts(data);
                });
        }

        // Update the RFID status and sensor data every 2 seconds
        setInterval(function () {
            updateSensorData(); // Update sensor data
            checkRFIDStatus(); // Check RFID status
        }, 2000);

        // Function to check RFID status
        function checkRFIDStatus() {
            // Make a request to the server to fetch RFID status
            fetch('/rfidstatus')
                .then(response => response.text())
                .then(status => {
                    // Update the RFID status element
                    document.getElementById('rfid-status-value').innerText = status;
                });
        }

        // Function to create and initialize the charts
        function initializeCharts() {
    // Create the Solar chart
    var solarCtx = document.getElementById('solarChart').getContext('2d');
    solarChart = new Chart(solarCtx, {
        type: 'line',
        data: {
            labels: ['Label1', 'Label2'], // Replace with your actual labels
            datasets: [
                {
                    label: 'Solar Voltage (V)',
                    borderColor: 'rgba(75, 192, 192, 1)',
                    borderWidth: 2,
                    data: [0.4, 0.8], // Replace with your actual data
                },
                {
                    label: 'Solar Current (A)',
                    borderColor: 'rgba(255, 99, 132, 1)',
                    borderWidth: 2,
                    data: [2.5, 3.0], // Replace with your actual data
                },
            ],
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    type: 'linear',
                },
                y: {
                    beginAtZero: true,
                },
            },
        },
    });


            // Create the Wind chart
            var windCtx = document.getElementById('windChart').getContext('2d');
            windChart = new Chart(windCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        {
                            label: 'Wind Voltage (V)',
                            borderColor: 'rgba(75, 192, 192, 1)',
                            borderWidth: 2,
                            data: [],
                        },
                        {
                            label: 'Wind Current (A)',
                            borderColor: 'rgba(255, 99, 132, 1)',
                            borderWidth: 2,
                            data: [],
                        },
                    ],
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        x: {
                            type: 'linear',
                        },
                        y: {
                            beginAtZero: true,
                        },
                    },
                },
            });
        }

        // Function to update chart data
        function updateCharts(data) {
            // Update the Solar chart
            solarChart.data.labels.push(new Date().toLocaleTimeString());
            solarChart.data.datasets[0].data.push(data.solarVoltage);
            solarChart.data.datasets[1].data.push(data.solarCurrent);

            // Update the Wind chart
            windChart.data.labels.push(new Date().toLocaleTimeString());
            windChart.data.datasets[0].data.push(data.windVoltage);
            windChart.data.datasets[1].data.push(data.windCurrent);

            // Remove old data if needed (e.g., to limit the number of data points shown)
            if (solarChart.data.labels.length > 10) {
                solarChart.data.labels.shift();
                solarChart.data.datasets[0].data.shift();
                solarChart.data.datasets[1].data.shift();
            }
            if (windChart.data.labels.length > 10) {
                windChart.data.labels.shift();
                windChart.data.datasets[0].data.shift();
                windChart.data.datasets[1].data.shift();
            }

            // Update the charts
            solarChart.update();
            windChart.update();
        }

        // Call the initializeCharts() function when the page loads
        document.addEventListener('DOMContentLoaded', function () {
            initializeCharts();
        });
    </script>
</body>
</html>
)=====";

const String AUTHORIZED_CARD_UID = "6A999B12";  // Replace with your card's UID

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void handleData() {
  float solarCurrent = analogRead(32) * (5.0 / 1023.0);
  float solarVoltage = analogRead(34) * (5.0 / 1023.0);
  float windCurrent = analogRead(35) * (5.0 / 1023.0);
  float windVoltage = analogRead(36) * (5.0 / 1023.0);

  // Calculate Solar Watt and Wind Watt
  float solarWatt = solarCurrent * solarVoltage;
  float windWatt = windCurrent * windVoltage;

  String json = "{\"solarCurrent\":" + String(solarCurrent, 2) + ",";
  json += "\"solarVoltage\":" + String(solarVoltage, 2) + ",";
  json += "\"solarWatt\":" + String(solarWatt, 2) + ",";  // Add Solar Watt
  json += "\"windCurrent\":" + String(windCurrent, 2) + ",";
  json += "\"windVoltage\":" + String(windVoltage, 2) + ",";
  json += "\"windWatt\":" + String(windWatt, 2) + "}";  // Add Wind Watt

  server.send(200, "application/json", json);
}


void handleRFIDStatus() {
  String status = "Access Granted";
  
  // Check if the switch is ON
  if (digitalRead(SWITCH_PIN) == HIGH) {
    status = "SCAN CARD";
  }
  
  server.send(200, "text/plain", status);
}

MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID module instance

void setup() {
  WiFi.softAP(ssid, password);
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.softAPIP());

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, HIGH);

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/rfidstatus", HTTP_GET, handleRFIDStatus);

  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfidCardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfidCardUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      rfidCardUID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    if (rfidCardUID == AUTHORIZED_CARD_UID) {
      digitalWrite(SWITCH_PIN, HIGH);
    } else {
      digitalWrite(SWITCH_PIN, LOW);
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}