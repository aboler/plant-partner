import express from "express";
import mongoose, { set } from "mongoose";
import dotenv from "dotenv";
import bodyParser from "body-parser";
import route from "./routes/sensorRoute.js";
import router from "./routes/plantRoute.js";
import cors from "cors";
import mqtt from "mqtt";
import http from "http";

// MQTT Client Setup
const MQTT_BROKER_URL = process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883';

const client = mqtt.connect(MQTT_BROKER_URL, {
    clientId: 'backend_server_' + Math.random().toString(16).substr(2, 8),
    clean: true,
    connectTimeout: 4000,
});

const topic = 'sensors/updates';

client.on('connect', () => {
    console.log('MQTT Client Connected');
});

function publishControlMessage(message) {
    client.publish(topic, message, { qos: 0, retain: false }, (error) => {
        if (error) {
            console.error('Publish error:', error);
        } else {
            console.log(`Control Message "${message}" published to topic "${topic}"`);
        }
    });
}

setTimeout(() => {
    publishControlMessage('TURN_ON');
    client.end();
}, 5000);

client.on('error', (error) => {
    console.error('MQTT Client Error:', error);
});

// Backend Server Setup
const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(express.json());
dotenv.config();

const PORT = process.env.PORT || 8000; //change to our desired port or if desired port changes
const MONGOURL = process.env.MONGO_URL || 'mongodb+srv://ncimino1_db_user:BKC89YKbMscuwW8A@plantpartnercluster.mfa1ope.mongodb.net/SensorDatabase?retryWrites=true&w=majority';

// MongoDB Connection

mongoose.connect(MONGOURL).then(() => {
    console.log('Connected to MongoDB')
    app.listen(PORT, () => {
        console.log(`Server running on port ${PORT}`);
    });
}).catch((error) => {
    console.error('Error connecting to MongoDB:', error);
});

app.use("/sensors", route);
app.use("/plants", router);