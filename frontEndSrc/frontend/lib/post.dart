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
    int v;

    Post({
        required this.id,
        required this.plantName,
        required this.soilMoisture,
        required this.lightIntensity,
        required this.nLevel,
        required this.pLevel,
        required this.kLevel,
        required this.v,
    });

    factory Post.fromJson(Map<String, dynamic> json) => Post(
        id: json["_id"],
        plantName: json["plantName"],
        soilMoisture: json["soilMoisture"]?.toDouble(),
        lightIntensity: json["lightIntensity"],
        nLevel: json["nLevel"],
        pLevel: json["pLevel"],
        kLevel: json["kLevel"],
        v: json["__v"],
    );

    Map<String, dynamic> toJson() => {
        "_id": id,
        "plantName": plantName,
        "soilMoisture": soilMoisture,
        "lightIntensity": lightIntensity,
        "nLevel": nLevel,
        "pLevel": pLevel,
        "kLevel": kLevel,
        "__v": v,
    };
}
