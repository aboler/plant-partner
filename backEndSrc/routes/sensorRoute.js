import express from "express"
import { fetchSensors, createSensor } from "../controller/sensorController.js";

const route = express.Router();

route.get("/fetchSensors", fetchSensors);
route.post("/createSensor", createSensor);

export default route;
