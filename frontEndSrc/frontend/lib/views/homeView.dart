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

  bool isSampling = false;
  bool isWatering = false;
  bool isLighting = false;
  bool isNutrients = false;

  String sampleStatus = "";
  String waterStatus = "";
  String lightStatus = "";
  String nutrientStatus = "";

  @override
  void initState() {
    super.initState();
    loadPlant();
  }

  Future<void> loadPlant() async {
    setState(() => isLoaded = false);
    plant = await RemoteService().getPlant();
    setState(() => isLoaded = true);
  }

  Future<void> triggerAllSensors() async {
    setState(() {
      isSampling = true;
      sampleStatus = "Actuating...";
    });

    final success = await RemoteService().triggerAllSensors();

    if (success) {
      await loadPlant();
    }

    setState(() {
      isSampling = false;
      sampleStatus = success ? "Sampling complete" : "Sampling failed";
    });

    await Future.delayed(Duration(seconds: 5));
    setState(() {
      sampleStatus = "";
    });
  }

  Future<void> lightMode() async {
    setState(() {
      isLighting = true;
      lightStatus = "Actuating...";
    });

    final success = await RemoteService().lightMode();

    setState(() {
      isLighting = false;
      lightStatus = success ? "Light triggered" : "Light trigger failed";
    });

    await Future.delayed(Duration(seconds: 5));
    setState(() {
      lightStatus = "";
    });
  }

  Future<void> triggerSensor(String sensor) async {
    if (sensor == "water") {
      setState(() {
        isWatering = true;
        waterStatus = "Actuating...";
      });

      final success = await RemoteService.triggerSensor(sensor);

      setState(() {
        isWatering = false;
        waterStatus = success ? "Water triggered" : "Water trigger failed";
      });
    } else if (sensor == "nutrients") {
      setState(() {
        isNutrients = true;
        nutrientStatus = "Actuating...";
      });

      final success = await RemoteService.triggerSensor(sensor);

      setState(() {
        isNutrients = false;
        nutrientStatus = success ? "Nutrients triggered" : "Nutrients trigger failed";
      });
    }

    await Future.delayed(Duration(seconds: 5));
    setState(() {
      waterStatus = "";
      nutrientStatus = "";
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text(
          'Home',
          style: TextStyle(fontWeight: FontWeight.bold),
        ),
        backgroundColor: Colors.lightGreen,
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: loadPlant,
          ),
        ],
      ),
      backgroundColor: const Color.fromARGB(255, 255, 248, 237),
      body: Center(
        child: !isLoaded
            ? const CircularProgressIndicator()
            : plant == null
                ? const Text("No plant data available")
                : SingleChildScrollView(
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                        PlantCard(plant: plant!),
                        const SizedBox(height: 20),

                        SensorButton(
                          icon: Icons.sensors,
                          label: "Sample",
                          color: Colors.green,
                          isRunning: isSampling,
                          statusText: sampleStatus,
                          onPressed: triggerAllSensors,
                        ),

                        SensorButton(
                          icon: Icons.water_drop_outlined,
                          label: "Water",
                          color: Colors.lightBlueAccent,
                          isRunning: isWatering,
                          statusText: waterStatus,
                          onPressed: () => triggerSensor("water"),
                        ),

                        SensorButton(
                          icon: Icons.lightbulb_outline_rounded,
                          label: "Light",
                          color: const Color.fromARGB(255, 251, 226, 145),
                          isRunning: isLighting,
                          statusText: lightStatus,
                          onPressed: lightMode,
                        ),

                        SensorButton(
                          icon: Icons.flare_outlined,
                          label: "Nutrients",
                          color: Colors.lightGreen,
                          isRunning: isNutrients,
                          statusText: nutrientStatus,
                          onPressed: () => triggerSensor("nutrients"),
                        ),
                      ],
                    ),
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
    final soilLevel = Plant.moistureLevel(plant.soilMoisture);
    final lightLevel = Plant.lightLevel(plant.lightIntensity);
    final nStatus = Plant.nutrientLevel(plant.nLevel);
    final pStatus = Plant.nutrientLevel(plant.pLevel);
    final kStatus = Plant.nutrientLevel(plant.kLevel);

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

            InfoRow(
              label: "Soil Moisture",
              value: "${plant.soilMoisture}",
              level: soilLevel,
            ),
            InfoRow(
              label: "Light Intensity",
              value: "${plant.lightIntensity}",
              level: lightLevel,
            ),
            const Divider(),
            InfoRow(
              label: "Nitrogen (N)",
              value: "${plant.nLevel}",
              level: nStatus,
            ),
            InfoRow(
              label: "Phosphorus (P)",
              value: "${plant.pLevel}",
              level: pStatus,
            ),
            InfoRow(
              label: "Potassium (K)",
              value: "${plant.kLevel}",
              level: kStatus,
            ),
          ],
        ),
      ),
    );
  }
}

class InfoRow extends StatelessWidget {
  final String label;
  final String value;
  final String level;

  const InfoRow({
    super.key,
    required this.label,
    required this.value,
    required this.level,
  });

  Color getLevelColor() {
    if (level == "Low") return Colors.orange;
    if (level == "Average") return Colors.green;
    return Colors.red;
  }

  IconData? getLevelIcon() {
    if (level == "Low") return Icons.warning_amber_rounded;
    return null;
  }

  @override
  Widget build(BuildContext context) {
    final levelColor = getLevelColor();
    final levelIcon = getLevelIcon();

    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 6),
      child: Column(
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                label,
                style: const TextStyle(
                  fontSize: 16,
                  fontWeight: FontWeight.w500,
                ),
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
          const SizedBox(height: 6),
          Row(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [
              if (levelIcon != null) ...[
                Icon(levelIcon, color: levelColor, size: 18),
                const SizedBox(width: 4),
              ],
              Container(
                padding: const EdgeInsets.symmetric(
                  horizontal: 10,
                  vertical: 4,
                ),
                decoration: BoxDecoration(
                  color: levelColor.withOpacity(0.15),
                  borderRadius: BorderRadius.circular(20),
                  border: Border.all(color: levelColor),
                ),
                child: Text(
                  level,
                  style: TextStyle(
                    color: levelColor,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class SensorButton extends StatelessWidget {
  final IconData icon;
  final String label;
  final Color color;
  final bool isRunning;
  final String statusText;
  final VoidCallback onPressed;

  const SensorButton({
    super.key,
    required this.icon,
    required this.label,
    required this.color,
    required this.isRunning,
    required this.statusText,
    required this.onPressed,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        ElevatedButton.icon(
          onPressed: isRunning ? null : onPressed,
          icon: isRunning
              ? const SizedBox(
                  height: 18,
                  width: 18,
                  child: CircularProgressIndicator(strokeWidth: 2),
                )
              : Icon(icon),
          label: Text(isRunning ? "$label..." : label),
          style: ElevatedButton.styleFrom(
            foregroundColor: Colors.black,
            backgroundColor: color,
            padding: const EdgeInsets.symmetric(horizontal: 24, vertical: 12),
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(12),
            ),
          ),
        ),
        if (statusText.isNotEmpty) ...[
          const SizedBox(height: 6),
          Text(
            statusText,
            style: TextStyle(
              fontSize: 13,
              color: statusText.toLowerCase().contains("failed")
                  ? Colors.red
                  : Colors.black87,
              fontWeight: FontWeight.w500,
            ),
          ),
        ],
        const SizedBox(height: 12),
      ],
    );
  }
}