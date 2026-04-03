import mongoose from "mongoose";

const taskSchema = new mongoose.Schema(
   {
      plantName: { type: String, required: true },
      type: { type: String, required: true },
      status: { type: String, default: "" },

      startDate: { type: String, required: true },
      endDate: { type: String, required: true },
      time: { type: String, required: true },
   },
   { timestamps: true }
);

export default mongoose.model("Task", taskSchema);
