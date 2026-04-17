import mongoose from "mongoose";

const taskSchema = new mongoose.Schema(
   {
      plantName: { type: String, required: true },
      taskTypes: { type: [String], required: true },

      startDate: { type: String, required: true },
      endDate: { type: String, required: true },
      time: { type: String, required: true },
      repeatDays: { type: [String], default: [] },
   },
   { timestamps: true }
);

export default mongoose.model("Task", taskSchema);
