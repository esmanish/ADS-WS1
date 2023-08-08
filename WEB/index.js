const express = require('express');
const mqtt = require('mqtt');
const cors = require('cors');
const http = require('http');
const WebSocket = require('ws'); // Import the WebSocket package

const app = express();
const PORT = 3000;

const mqttBroker = 'mqtt://10.100.83.226';
const topic = 'weather_data';

const client = mqtt.connect(mqttBroker);
const server = http.createServer(app);

const wss = new WebSocket.Server({ server }); // Create a WebSocket server

let weatherData = {};

client.on('connect', () => {
  console.log('Connected to MQTT broker');
  client.subscribe(topic);
});

client.on('message', (topic, message) => {
  try {
    weatherData = JSON.parse(message.toString());
    
    // Send the updated weather data to all connected WebSocket clients
    wss.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(weatherData));
      }
    });
  } catch (error) {
    console.error('Error parsing MQTT message:', error);
  }
});

app.use(cors());

app.get('/api/weather', (req, res) => {
  res.json(weatherData);
});

wss.on('connection', ws => {
  console.log('WebSocket connected');
  
  // Send initial weather data when a client connects
  ws.send(JSON.stringify(weatherData));
  
  ws.on('close', () => {
    console.log('WebSocket disconnected');
  });
});

server.listen(PORT, () => {
  console.log(`Server listening on port ${PORT}`);
});
