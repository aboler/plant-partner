import 'package:flutter/material.dart';
import 'package:frontend/plant.dart';
import 'package:frontend/services/remote_service.dart';

class TaskView extends StatefulWidget {
  const TaskView({super.key});

  @override
  State<TaskView> createState() => _TaskViewState();
}

class _TaskViewState extends State<TaskView> {
  Plant? plant;
  bool loading = true;

  @override
  void initState() {
    super.initState();
    loadPlant();
  }

  Future<void> loadPlant() async {
    plant = await RemoteService().getPlant();
    setState(() => loading = false);
  }

  Future<void> toggleAuto(bool value) async {
    setState(() => loading = true);
    await RemoteService().setAutoSchedule(value);
    await loadPlant();
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(
          value ? "Auto scheduling enabled" : "Auto scheduling disabled",
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightGreen,
        title: const Text("Scheduling"),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: loadPlant,
          ),
        ],
      ),
      backgroundColor: Colors.amber.shade50,
      body: loading
          ? const Center(child: CircularProgressIndicator())
          : plant == null
              ? const Center(child: Text("No plant data"))
              : Padding(
                  padding: const EdgeInsets.all(16),
                  child: Card(
                    shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(14)),
                    child: Padding(
                      padding: const EdgeInsets.all(16),
                      child: Column(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          const Text(
                            "Automation",
                            style: TextStyle(
                                fontSize: 20, fontWeight: FontWeight.bold),
                          ),
                          const SizedBox(height: 12),
                          SwitchListTile(
                            title: const Text("Enable Auto Scheduling"),
                            value: plant!.autoSchedule,
                            onChanged: toggleAuto,
                          ),
                        ],
                      ),
                    ),
                  ),
                ),
    );
  }
}
