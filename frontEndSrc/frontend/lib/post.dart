//to parse this JSON data, do
//final post = postFromJson(jsonString);

import 'dart:convert';

Post postFromJson(String str) => Post.fromJson(json.decode(str));

String postToJson(Post data) => json.encode(data.toJson());

class Post {
    String id;
    String plantName;
    double soilMoisture;
    int lightIntensity;
    int nLevel;
    int pLevel;
    int kLevel;

    Post({
        this.id = '',
        this.plantName = '',
        this.soilMoisture = 0.0,
        this.lightIntensity = 0,
        this.nLevel = 0,
        this.pLevel = 0,
        this.kLevel = 0,
    });

    factory Post.fromJson(Map<String, dynamic> json) => Post(
        id: json["_id"],
        plantName: json["plantName"],
        soilMoisture: json["soilMoisture"]?.toDouble(),
        lightIntensity: json["lightIntensity"],
        nLevel: json["nLevel"],
        pLevel: json["pLevel"],
        kLevel: json["kLevel"],
    );

    Map<String, dynamic> toJson() => {
        "_id": id,
        "plantName": plantName,
        "soilMoisture": soilMoisture,
        "lightIntensity": lightIntensity,
        "nLevel": nLevel,
        "pLevel": pLevel,
        "kLevel": kLevel,
    };
}