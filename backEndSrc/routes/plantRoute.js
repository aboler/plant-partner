import express from "express"
import { createPlant, fetchPlants, getPlantByName, updatePlantByName, deletePlantByName } from "../controller/plantController.js";
import { updateAutoScheduleByName } from "../controller/plantController.js";

const router = express.Router();

router.post("/createPlant", createPlant);
router.get("/fetchPlants", fetchPlants);
router.get("/getPlantByName/:name", getPlantByName);
router.put("/updatePlantByName/:name", updatePlantByName);
router.delete("/deletePlantByName/:name", deletePlantByName);
router.put("/updateAutoSchedule/:name", updateAutoScheduleByName);

export default router;