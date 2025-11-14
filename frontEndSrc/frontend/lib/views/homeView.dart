import 'package:flutter/material.dart';
import 'package:frontend/plant.dart';
import 'package:frontend/services/remote_service.dart';

class HomeView extends StatefulWidget {
  const HomeView({super.key});

  @override
  State<HomeView> createState() => _HomeViewState();
}

class _HomeViewState extends State<HomeView> {
  Plant? plant;
  bool isLoaded = false;

  @override
  void initState() {
    super.initState();
    loadPlant();
  }

  //fetch latest plant data from backend
  loadPlant() async {
    setState(() => isLoaded = false); //show loading spinner again
    plant = await RemoteService().getPlant();
    setState(() => isLoaded = true);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Home'),
        backgroundColor: Colors.lightGreen,
      ),
      backgroundColor: const Color.fromARGB(255, 255, 248, 237),

      body: Center(
        child: !isLoaded
            ? const CircularProgressIndicator()
            : plant == null
                ? const Text("No plant data available")
                : Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      PlantCard(plant: plant!),
                      const SizedBox(height: 20),

                      ElevatedButton.icon(
                        onPressed: () {
                          loadPlant();
                        },
                        icon: const Icon(Icons.refresh),
                        label: const Text("Refresh Data"),
                        style: ElevatedButton.styleFrom(
                          backgroundColor: Colors.lightGreen,
                          padding: const EdgeInsets.symmetric(
                              horizontal: 24, vertical: 12),
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(12),
                          ),
                        ),
                      ),
                    ],
                  ),
      ),
    );
  }
}

class PlantCard extends StatelessWidget {
  final Plant plant;
  const PlantCard({super.key, required this.plant});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(14)),
      margin: const EdgeInsets.all(18),
      child: Padding(
        padding: const EdgeInsets.all(18.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisSize: MainAxisSize.min,
          children: [
            Center(
              child: Icon(
                Icons.local_florist,
                size: 80,
                color: Colors.green.shade600,
              ),
            ),
            const SizedBox(height: 12),

            Center(
              child: Text(
                plant.plantName,
                style: const TextStyle(
                  fontSize: 22,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),

            const SizedBox(height: 20),
            InfoRow(label: "Soil Moisture", value: "${plant.soilMoisture}"),
            InfoRow(label: "Light Intensity", value: "${plant.lightIntensity}"),
            const Divider(),
            InfoRow(label: "Nitrogen (N)", value: "${plant.nLevel}"),
            InfoRow(label: "Phosphorus (P)", value: "${plant.pLevel}"),
            InfoRow(label: "Potassium (K)", value: "${plant.kLevel}"),
          ],
        ),
      ),
    );
  }
}

class InfoRow extends StatelessWidget {
  final String label;
  final String value;

  const InfoRow({
    super.key,
    required this.label,
    required this.value,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 6),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(
            label,
            style:
                const TextStyle(fontSize: 16, fontWeight: FontWeight.w500),
          ),
          Text(
            value,
            style: TextStyle(
              fontSize: 16,
              color: Colors.green.shade700,
            ),
          ),
        ],
      ),
    );
  }
}