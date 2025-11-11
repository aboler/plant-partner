import express from "express"
import { createPlant, fetchPlants, getPlantByName, updatePlantByName, deletePlantByName } from "../controller/plantController.js";

const router = express.Router();

router.post("/", createPlant);
router.get("/", fetchPlants);
router.get("/:name", getPlantByName);
router.put("/:name", updatePlantByName);
router.delete("/:name", deletePlantByName);

export default router;