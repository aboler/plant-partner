import express from "express";
import { createTask, getTasksByPlant, markTaskDone, updateTask, deleteTask } from "../controller/taskController.js";

const router = express.Router();

router.post("/createTask", createTask);
router.get("/getTasks/:plantName", getTasksByPlant);
router.put("/markDone/:id", markTaskDone);
router.put("/updateTask/:id", updateTask);
router.delete("/deleteTask/:id", deleteTask);

export default router;
