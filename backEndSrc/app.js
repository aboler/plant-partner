import express from "express";
import mongoose from "mongoose";
import dotenv from "dotenv";
import bodyParser from "body-parser";
import route from "./routes/sensorRoute.js";
import cors from "cors";
import mqtt from "mqtt";

// MQTT Broker Setup
const MQTT_BROKER_URL = process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883';
const mqttClient = mqtt.connect(MQTT_BROKER_URL);

mqttClient.on('connect', () => {
    console.log('Connected to MQTT Broker');
    mqttClient.subscribe('sensors/updates', (err) => {
        if (!err) {
            console.log('Subscribed to sensors/updates topic');
        } else {
            console.error('Subscription error:', err);
        }
    });
});

// Backend Server Setup
const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(express.json());
dotenv.config();

const PORT = process.env.PORT || 8000; //change to our desired port or if desired port changes
const MONGOURL = process.env.MONGO_URL || 'mongodb://localhost:27017/SensorDatabase';


mongoose.connect(MONGOURL).then(() => {
    console.log('Connected to MongoDB')
    app.listen(PORT, () => {
        console.log(`Server running on port ${PORT}`);
    });
}).catch((error) => {
    console.error('Error connecting to MongoDB:', error);
});

app.use("/api/sensor", route);