//to parse this JSON data, do
//final post = postFromJson(jsonString);

import 'dart:convert';

Post postFromJson(String str) => Post.fromJson(json.decode(str));

String postToJson(Post data) => json.encode(data.toJson());

class Post {
    String plantName;
    int soilMoisture;
    int lightIntensity;
    int nLevel;
    int pLevel;
    int kLevel;

    Post({
        required this.plantName,
        required this.soilMoisture,
        required this.lightIntensity,
        required this.nLevel,
        required this.pLevel,
        required this.kLevel,
    });

    factory Post.fromJson(Map<String, dynamic> json) => Post(
        plantName: json["plantName"],
        soilMoisture: json["soilMoisture"],
        lightIntensity: json["lightIntensity"],
        nLevel: json["nLevel"],
        pLevel: json["pLevel"],
        kLevel: json["kLevel"],
    );

    Map<String, dynamic> toJson() => {
        "plantName": plantName,
        "soilMoisture": soilMoisture,
        "lightIntensity": lightIntensity,
        "nLevel": nLevel,
        "pLevel": pLevel,
        "kLevel": kLevel,
    };
}