class Post{
  //received data: "[{\"_id\":\"68fed1313fbfd4545947b6df\",\"name\":\"MoistureSensor\",\"time\":10,\"value\":25,\"__v\":0}]"
  //temp variables for the plant posts

  //however if its optional itll be datatype? varnam
  String id;
  String name;
  int time;
  int value;

  Post({
    //required need property
    required this.id,
    required this.name,
    required this.time,
    required this.value,
  });
}