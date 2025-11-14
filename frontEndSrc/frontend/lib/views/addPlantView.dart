import 'package:flutter/material.dart';

class AddPlantView extends StatelessWidget {
  const AddPlantView({super.key});

  @override
  Widget build(BuildContext context) {
    final TextEditingController nameCtrl = TextEditingController();

    return Scaffold(
      appBar: AppBar(title: const Text("Add New Plant"), backgroundColor: Colors.lightGreen),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const Icon(Icons.image, size: 120, color: Colors.grey),
            const SizedBox(height: 8),
            TextField(
              controller: nameCtrl,
              decoration: const InputDecoration(labelText: "Name Plant"),
            ),
            const SizedBox(height: 16),
            ElevatedButton(
              onPressed: () {
                Navigator.pop(context);
              },
              style: ElevatedButton.styleFrom(backgroundColor: Colors.lightGreen),
              child: const Text("Add Plant"),
            ),
          ],
        ),
      ),
    );
  }
}
