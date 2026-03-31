import express from "express";
import { createTask, getTasksByPlant, markTaskDone } from "../controller/taskController.js";

const router = express.Router();

router.post("/createTask", createTask);
router.get("/getTasks/:plantName", getTasksByPlant);
router.put("/markDone/:id", markTaskDone);

export default router;
