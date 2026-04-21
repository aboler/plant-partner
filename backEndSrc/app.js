import express from "express";
import mongoose from "mongoose";
import dotenv from "dotenv";
import bodyParser from "body-parser";
import route from "./routes/sensorRoute.js";
import router from "./routes/plantRoute.js";
import taskRouter from "./routes/taskRoute.js";
import Task from "./model/taskModel.js";
import Plant from "./model/plantModel.js";
import cors from "cors";
import mqtt from "mqtt";

//Functions for reading control variables 
async function readControlVar(client) {
    const taskCursor = Task.find().cursor();
    const date = new Date();
    for (let t = await taskCursor.next(); t != null; t = await taskCursor.next()) {      
        const timeStr = t.time + ':00';
        const comp_date = new Date(t.endDate + 'T' + timeStr);
        const act_period = new Date(date.toISOString().substring(0, 10) + 'T' + timeStr);
        //need variables to get the current weekday and variables to get the current time (not datetime, just time, military)

        let current_day = "Null";

        switch (date.getDay()) {
            case 0:
                current_day = "Su";
                break;
            case 1:
                current_day = "M";
                break;
            case 2:
                current_day = "T";
                break;
            case 3:
                current_day = "W";
                break;
            case 4:
                current_day = "Th";
                break;
            case 5:
                current_day = "F";
                break;
            case 6:
                current_day = "Sa";
                break;
            default:
                console.log('Error: Failed to get Current Weekday.');
                break;
        }

        //Check if task is on its time stamp and on a correct weekday in order to execute the task
        if (((act_period.getTime() - (date.getTime() - 30100)) >= 0) && ((date.getTime() - act_period.getTime()) >= 0) && (t.repeatDays.includes(current_day))) {
            try {
                let task_list = "T:";
                for (const task of t.taskTypes) {
                    task_list = task_list + task + " ";
                }
                client.publish('plant_partner/task', task_list);
                console.log('Successful Actuation Request');
            } catch (err) {
                console.log('ERROR: Missed Actuation Request');
            }
        }

        // Check if the task is on its end date and time in order to delete it
        if (((comp_date.getTime()-(date.getTime() - 30100)) >= 0) && ((date.getTime() - comp_date.getTime()) >= 0)) {
            try {
                await t.deleteOne();
                console.log('Task deleted');
            } catch (err) {
                console.log('ERROR: Task NOT deleted');
            }
        }
    }
}

async function autoSample(client) {
    try {
        client.publish('plant_partner/task', 'default');
        client.publish('plant_partner/auto_error_notif', 'autosampling completed');
        console.log('suucessful sampling');
    } catch (err) {
        console.log('ERROR: Unsuccessful Default Request');
    }
}
//

// MQTT Broker Setup
const MQTT_BROKER_URL = process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883' // 'mqtt://localhost:1883'; mqtt://test.mosquitto.org:1883 // CHANGE IP !!
const mqttClient = mqtt.connect(MQTT_BROKER_URL);
const ACT_INTERVAL_MS = 30000; // 20 seconds
const SAMPLE_INTERVAL_MS = 47000; // 47 seconds

mqttClient.on('connect', async () => {

    //On startup, send message to micro of current value of autoschedule 
    const currentPlant = await Plant.findOne();
    mqttClient.publish("plant_partner/autocare_startup", String(currentPlant.autoSchedule));

    //subscribe to topics
    mqttClient.subscribe("plant_partner/auto_en");
    mqttClient.subscribe("plant_partner/esp_startup");
    mqttClient.subscribe("plant_partner/auto_notif");
    mqttClient.subscribe("plant_partner/act_compl");

    //tasks and data recording 
    setInterval(() => {
        readControlVar(mqttClient);
    }, ACT_INTERVAL_MS);

    setInterval(() => {
        autoSample(mqttClient);
    }, SAMPLE_INTERVAL_MS);
    
});

mqttClient.on('message', async (topic, message) => {
    console.log(`Received message on topic ${topic}: ${message.toString()}`);
    // Here you can add logic to process the message and update the database if needed

    switch (topic) {
        case "plant_partner/auto_en" :
            //for toggling autocare on esp32
            mqttClient.publish("plant_partner/act_tog_en", message);
            break;
        case "plant_partner/esp_startup" :
            //for getting the current autocare status and sending to esp32 on its reset
            const currentPlant = await Plant.findOne();
            mqttClient.publish("plant_partner/autocare_startup", String(currentPlant.autoSchedule));
            break;
        case "plant_partner/act_compl" :
            //For passing along messages to frontend when actuation task is completed, it could potentailly receive errors if actuation failed
            mqttClient.publish("plant_partner/act_notif", message);
            break;
        case "plant_partner/auto_notif" :
            mqttClient.publish("plant_partner/auto_error_notif", message);
            break;
        default :
            break;

    }

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

// 