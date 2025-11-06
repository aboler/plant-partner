import 'package:flutter/material.dart';

class PlantDetailsView extends StatelessWidget {
  final String plantName;
  const PlantDetailsView({super.key, required this.plantName});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text(plantName), backgroundColor: Colors.lightGreen),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          children: [
            const Icon(Icons.eco, size: 100, color: Colors.green),
            const SizedBox(height: 16),
            const Text("Plant Details", style: TextStyle(fontWeight: FontWeight.bold)),
            const SizedBox(height: 20),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: const [
                Text("Water:"),
                Text("Low"),
              ],
            ),
            const Divider(),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: const [
                Text("Nutrients:"),
                Text("Optimal"),
              ],
            ),
            const Spacer(),
            ElevatedButton.icon(
              onPressed: () {},
              icon: const Icon(Icons.water_drop),
              label: const Text("Add Water"),
              style: ElevatedButton.styleFrom(backgroundColor: Colors.lightGreen),
            ),
          ],
        ),
      ),
    );
  }
}
