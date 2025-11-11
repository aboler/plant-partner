import express from "express"
import { createPlant, fetchPlants, getPlantByName, updatePlantByName, deletePlantByName } from "../controller/plantController.js";

const router = express.Router();

router.post("/createPlant", createPlant);
router.get("/fetchPlants", fetchPlants);
router.get("/getPlantByName/:name", getPlantByName);
router.put("/updatePlantByName/:name", updatePlantByName);
router.delete("/deletePlantByName/:name", deletePlantByName);

export default router;