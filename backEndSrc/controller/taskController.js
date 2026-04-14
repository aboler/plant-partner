import Task from "../model/taskModel.js";

export const createTask = async (req, res) => {
   try {
      const { plantName, type, status, startDate, endDate, time, repeatDays } = req.body;

      if (!plantName || !type || !startDate || !endDate || !time) {
         return res.status(400).json({ error: "Missing required task fields" });
      }

      const taskData = new Task({
         plantName,
         type,
         status: status || "pending",
         startDate,
         endDate,
         time,
         repeatDays: repeatDays || [],
      });

      const savedTask = await taskData.save();
      res.status(201).json(savedTask);
   } catch (error) {
      res.status(400).json({ error: "Failed to create task" });
   }
};

export const getTasksByPlant = async (req, res) => {
   try {
      const plantName = req.params.plantName;
      const tasks = await Task.find({ plantName }).sort({ createdAt: -1 });
      res.status(200).json(tasks);
   } catch (error) {
      res.status(500).json({ error: "Failed to fetch tasks" });
   }
};

export const markTaskDone = async (req, res) => {
   try {
      const id = req.params.id;
      const task = await Task.findById(id);

      if (!task) {
         return res.status(404).json({ message: "Task not found" });
      }

      task.status = "done";
      await task.save();

      res.status(200).json(task);
   } catch (error) {
      res.status(500).json({ error: "Failed to update task" });
   }
};
