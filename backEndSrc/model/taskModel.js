import mongoose from "mongoose";

const taskSchema = new mongoose.Schema(
   {
      plantName: { type: String, required: true },
      type: { type: String, required: true },
      status: { type: String, default: "" },
   },
   { timestamps: true }
);

export default mongoose.model("Task", taskSchema);
