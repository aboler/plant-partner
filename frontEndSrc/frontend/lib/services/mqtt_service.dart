import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:frontend/main.dart';

class MqttService {
  MqttServerClient? _client;
  final String _identifier;
  final String _host;
  final String _topic;

  MqttService({
    required String identifier,
    required String host,
    required String topic,
  })  : _identifier = identifier,
        _host = host,
        _topic = topic;

  Future<void> initMqttClient() async {
    _client = MqttServerClient(_host, _identifier);

    _client!.port = 1883;
    _client!.keepAlivePeriod = 20;
    _client!.logging(on: true);
    _client!.setProtocolV311();

    _client!.onConnected = onConnected;
    _client!.onDisconnected = onDisconnected;
    _client!.onSubscribed = onSubscribed;

    final connMessage = MqttConnectMessage()
        .withClientIdentifier(_identifier)
        .startClean()
        .withWillQos(MqttQos.atLeastOnce);

    _client!.connectionMessage = connMessage;

    try {
      print('connecting to mqtt broker...');
      await _client!.connect();
    } catch (e) {
      print('mqtt connection failed: $e');
      _client!.disconnect();
      return;
    }

    if (_client!.connectionStatus?.state == MqttConnectionState.connected) {
      print('mqtt connected');

      _client!.subscribe("plant_partner/act_notif", MqttQos.atLeastOnce);
      _client!.subscribe("plant_partner/auto_error_notif", MqttQos.atLeastOnce);

      print('subscribed to the topics');

      _client!.updates!.listen((List<MqttReceivedMessage<MqttMessage>> events) {
        final recMess = events[0].payload as MqttPublishMessage;
        final topic = events[0].topic;

        final payload = MqttPublishPayload.bytesToStringAsString(
          recMess.payload.message,
        );

        print('message received');
        print('topic: $topic');
        print('payload: $payload');

        _showNotification(topic, payload);
      });
    } else {
      print('mqtt connection status: ${_client!.connectionStatus}');
      _client!.disconnect();
    }
  }

  void onConnected() {
    print('connected');
  }

  void onDisconnected() {
    print('disconnected');
  }

  void onSubscribed(String topic) {
    print('subscribed to $topic');
  }

  void _showNotification(String topic, String message) {
    String title;

    if (topic == "plant_partner/act_notif") {
      title = "plant action notification";
    } else if (topic == "plant_partner/auto_error_notif") {
      title = "auto care error";
    } else {
      title = "mqtt message";
    }

    scaffoldMessengerKey.currentState?.showSnackBar(
      SnackBar(
        content: Text('$title: $message'),
        duration: const Duration(seconds: 4),
        behavior: SnackBarBehavior.floating,
      ),
    );
  }

  void disconnect() {
    _client?.disconnect();
  }
}