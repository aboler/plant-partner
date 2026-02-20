import express from "express";
import mongoose from "mongoose";
import dotenv from "dotenv";
import bodyParser from "body-parser";
import route from "./routes/sensorRoute.js";
import router from "./routes/plantRoute.js";
import taskRouter from "./routes/taskRoute.js";
import Task from "./model/taskModel.js";
import cors from "cors";
import mqtt from "mqtt";

//Functions for reading control variables 
async function readControlVar(client) {
    const taskCursor = Task.find().cursor();
    for (let t = await taskCursor.next(); t != null; t = await taskCursor.next()) {      
        console.log(t.type);
        if(t.status == 'pending') {
            client.publish('plant_partner/ack', t.type, (err) => {
                if(!err) {
                    console.log('Successful Actuation Request');
                } else {
                    console.log('ERROR: Missed Actuation Request');
                } 
            });
        } 
        try {
            await t.deleteOne();
            console.log('Task deleted');
        } catch (err) {
            console.log('ERROR: Task NOT deleted');
        }
    }
    client.publish('plant_partner/ack', 'default', (err) => {
        if(!err) {
            console.log('Successful Default Request');
        } else {
            console.log('ERROR: Unsuccessful Default Request')
        }
    });
}

// MQTT Broker Setup
const MQTT_BROKER_URL = process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883';
const mqttClient = mqtt.connect(MQTT_BROKER_URL);
//const SAMPLE_INTERVAL_MS = 30000; // 30 seconds
const ACT_INTERVAL_MS = 60000; // 60 seconds, 1 minute

mqttClient.on('connect', () => {

    //tasks and data recording 
    setInterval(() => {
        readControlVar(mqttClient);
    }, ACT_INTERVAL_MS);
    
});

mqttClient.on('message', (topic, message) => {
    console.log(`Received message on topic ${topic}: ${message.toString()}`);
    // Here you can add logic to process the message and update the database if needed
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
app.use("/tasks", taskRouter);