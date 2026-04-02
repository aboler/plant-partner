import 'package:flutter/material.dart';
import 'package:frontend/nav.dart';
import 'package:frontend/routes.dart';
import 'package:frontend/services/mqtt_service.dart';

final GlobalKey<ScaffoldMessengerState> scaffoldMessengerKey =
    GlobalKey<ScaffoldMessengerState>();

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  late MqttService mqttService;

  @override
  void initState() {
    super.initState();

    mqttService = MqttService(
      identifier: 'flutter_client_1',
      host: 'test.mosquitto.org', //adjust based on what broker were using
      topic: 'plant_partner/#',
    );

    mqttService.initMqttClient();
  }

  @override
  void dispose() {
    mqttService.disconnect();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      scaffoldMessengerKey: scaffoldMessengerKey,
      routes: appRoutes,
      home: Nav(),
      theme: ThemeData(
        elevatedButtonTheme: ElevatedButtonThemeData(
          style: ElevatedButton.styleFrom(
            foregroundColor: Colors.black,
            padding: const EdgeInsets.symmetric(horizontal: 24, vertical: 12),
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(12),
            ),
          ),
        ),
      ),
    );
  }
}