import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:frontend/plant.dart';
import 'package:frontend/task.dart';
import 'dart:convert';

import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

const String baseUrl = 'http://127.0.0.1:8000/plants/getPlantByName/Sunflower';
//const String baseUrl = 'http://:8000/plants/getPlantByName/Sunflower';

class RemoteService {
  static const String url = "http://127.0.0.1:8000";
  //static const String url = "http://:8000";

  Future<Plant?> getPlant() async {
    final resp = await http.get(Uri.parse(baseUrl));

    if (resp.statusCode == 200) {
      print('made it thru');
      final List data = json.decode(resp.body);

      if (data.isNotEmpty) {
        return Plant.fromJson(data[0]);
      }
    } else {
      print('failed: ${resp.statusCode}');
    }

    return null;
  }

  Future<void> triggerAllSensors() async {
    final client = MqttServerClient("127.0.0.1", "flutter_client_1");
    client.port = 1883;
    client.keepAlivePeriod = 20;

    await client.connect();

    final builder = MqttClientPayloadBuilder();
    builder.addString("Triggering all sensors");

    client.publishMessage(
      "plant_partner/act_tog_en",
      MqttQos.atLeastOnce,
      builder.payload!,
    );

    client.disconnect();
}

  Future<bool> setAutoSchedule(bool enabled) async {
    final resp = await http.put(
      Uri.parse("$url/plants/updateAutoSchedule/Sunflower"),
      headers: {'Content-Type': 'application/json'},
      body: jsonEncode({'autoSchedule': enabled}),
    );

    return resp.statusCode == 200;
  }

  // gets all tasks for the plant
  Future<List<Task>> getTasks() async {
    final resp = await http.get(
      Uri.parse("$url/tasks/getTasks/Sunflower"),
    );

    if (resp.statusCode == 200) {
      final List data = json.decode(resp.body);
      return data.map((e) => Task.fromJson(e)).toList();
    } else {
      print('failed getTasks: ${resp.statusCode}');
      return [];
    }
  }

  // creates a new task (start with type="water")
  Future<bool> createTask(String type) async {
    final resp = await http.post(
      Uri.parse("$url/tasks/createTask"),
      headers: {'Content-Type': 'application/json'},
      body: jsonEncode({
        'plantName': 'Sunflower',
        'type': type,
        'status': 'pending',
      }),
    );

    if (resp.statusCode != 200) {
      print('failed createTask: ${resp.statusCode}');
    }

    return resp.statusCode == 200;
  }

  // marks a task as done
  Future<bool> markTaskDone(String id) async {
    final resp = await http.put(
      Uri.parse("$url/tasks/markDone/$id"),
    );

    if (resp.statusCode != 200) {
      print('failed markTaskDone: ${resp.statusCode}');
    }

    return resp.statusCode == 200;
  }
}
