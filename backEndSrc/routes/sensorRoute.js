import express from "express"
import { fetchSensors, createSensor, updateSensor } from "../controller/sensorController.js";

const route = express.Router();

route.get("/fetchSensors", fetchSensors);
route.post("/createSensor", createSensor);
route.put("/updateSensor/:id", updateSensor);

export default route;
