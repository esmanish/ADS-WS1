const apiUrl = 'http://10.100.83.226:3000/api/weather';
const socketUrl = 'ws://10.100.83.226:3000'; // WebSocket URL

const rainAmountElement = document.getElementById('rain-amount');
const windSpeedElement = document.getElementById('wind-speed');
const windDirectionElement = document.getElementById('wind-direction');
const pressureElement = document.getElementById('pressure');
const averageTemperatureElement = document.getElementById('average-temperature');

let socket = null; // Declare the socket variable

// Function to establish WebSocket connection
function establishWebSocketConnection() {
    socket = new WebSocket(socketUrl);

    socket.addEventListener('open', () => {
        console.log('WebSocket connection established');
    });

    socket.addEventListener('message', event => {
        const data = JSON.parse(event.data);
        // Update your UI with new data here
        updateWeatherUI(data);
    });

    socket.addEventListener('close', event => {
        console.log('WebSocket connection closed');
        // Reconnect after a short delay
        setTimeout(establishWebSocketConnection, 2000);
    });
}

// Function to update weather UI
function updateWeatherUI(data) {
    rainAmountElement.textContent = `Rain Amount: ${data.rainAmount} mm`;
    windSpeedElement.textContent = `Wind Speed: ${data.windSpeed} m/s`;
    windDirectionElement.textContent = `Wind Direction: ${data.windDirection}°`;
    pressureElement.textContent = `Pressure: ${data.pressure} hPa`;
    averageTemperatureElement.textContent = `Average Temperature: ${data.averageTemperature}°C`;
}

// Call the fetchWeatherData function to get weather data on page load
async function fetchWeatherData() {
    try {
        const response = await fetch(apiUrl);
        const data = await response.json();
        updateWeatherUI(data); // Update UI using fetched data
    } catch (error) {
        console.error('Error fetching weather data:', error);
    }
}

// Start WebSocket connection
establishWebSocketConnection();

// Call the fetchWeatherData function to get weather data on page load
fetchWeatherData();
