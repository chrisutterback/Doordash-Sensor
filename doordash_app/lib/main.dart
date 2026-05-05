import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

// 1. Create a global instance
final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
FlutterLocalNotificationsPlugin();

void main() async {
  WidgetsFlutterBinding.ensureInitialized();

  const AndroidInitializationSettings initializationSettingsAndroid =
  AndroidInitializationSettings('@mipmap/ic_launcher');

  const DarwinInitializationSettings initializationSettingsIOS =
  DarwinInitializationSettings(
    requestAlertPermission: true,
    requestBadgePermission: true,
    requestSoundPermission: true,
    defaultPresentAlert: true,
    defaultPresentBadge: true,
    defaultPresentSound: true,
    defaultPresentBanner: true,
    defaultPresentList: true,
  );

  const InitializationSettings initializationSettings = InitializationSettings(
      android: initializationSettingsAndroid,
      iOS: initializationSettingsIOS);

  await flutterLocalNotificationsPlugin.initialize(initializationSettings);

  await flutterLocalNotificationsPlugin
      .resolvePlatformSpecificImplementation<
      IOSFlutterLocalNotificationsPlugin>()
      ?.requestPermissions(
    alert: true,
    badge: true,
    sound: true,
  );

  runApp(const MyApp());
}

Future<void> showTriggerAlert(String company) async {
  const NotificationDetails details = NotificationDetails(
    android: AndroidNotificationDetails(
      'esp_channel',
      'ESP32 Alerts',
      importance: Importance.max,
      priority: Priority.high,
    ),
    iOS: DarwinNotificationDetails(
      presentAlert: true,
      presentBadge: true,
      presentSound: true,
    ),
  );

  await flutterLocalNotificationsPlugin.show(
    DateTime.now().millisecondsSinceEpoch ~/ 1000,
    'Delivery Update',
    "$company is delivered!",
    details,
  );
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Delivery Tracker',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
      ),
      home: const MyHomePage(title: 'Delivery Tracker'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  final _urlController = TextEditingController(text: 'ws://172.20.10.12:81');
  WebSocketChannel? _channel;
  StreamSubscription? _subscription;

  String _esp32Data = "Waiting for ESP32...";
  String _connectionStatus = "Disconnected";
  String? _lastError;
  bool _isConnected = false;

  List<Map<String, dynamic>> deliveries = [
    {"name": "DoorDash", "delivered": false, "image": "assets/images/DoorDash.png"},
    {"name": "Uber Eats", "delivered": false, "image": "assets/images/uberEats.png"},
    {"name": "Amazon", "delivered": false, "image": "assets/images/Amazon.png"},
  ];

  @override
  void initState() {
    super.initState();
    _connectToEsp32();
  }

  Future<void> _connectToEsp32() async {
    final url = _urlController.text.trim();
    if (url.isEmpty) {
      setState(() {
        _connectionStatus = 'Disconnected';
        _lastError = 'Please enter your ESP32 WebSocket URL.';
      });
      return;
    }

    _subscription?.cancel();
    _channel?.sink.close();

    try {
      setState(() {
        _connectionStatus = 'Connecting...';
        _lastError = null;
        _esp32Data = 'Connecting to $url';
      });

      _channel = WebSocketChannel.connect(Uri.parse(url));
      _subscription = _channel!.stream.listen(
        (message) {
          final raw = message.toString();
          final parts = raw.split(':');

          setState(() {
            _isConnected = true;
            _connectionStatus = 'Connected';
            _esp32Data = raw;

            if (parts.length >= 2 && parts[0] == 'DELIVERED') {
              final company = parts[1].trim();
              final index = deliveries.indexWhere(
                (d) => d['name'].toString().toLowerCase() == company.toLowerCase(),
              );
              if (index != -1) {
                deliveries[index]['delivered'] = true;
              }
            }
          });

          if (parts.length >= 2 && parts[0] == 'DELIVERED') {
            showTriggerAlert(parts[1].trim());
          }
        },
        onError: (error) {
          setState(() {
            _connectionStatus = 'Error';
            _lastError = error.toString();
            _isConnected = false;
          });
        },
        onDone: () {
          setState(() {
            _connectionStatus = 'Disconnected';
            _isConnected = false;
          });
        },
      );
    } catch (error) {
      setState(() {
        _connectionStatus = 'Disconnected';
        _lastError = error.toString();
        _isConnected = false;
        _esp32Data = 'Waiting for ESP32...';
      });
    }
  }

  @override
  void dispose() {
    _subscription?.cancel();
    _channel?.sink.close();
    _urlController.dispose();
    super.dispose();
  }

  void markDelivered(int index) {
    setState(() {
      deliveries[index]['delivered'] = true;
    });
    showTriggerAlert(deliveries[index]['name']);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Delivery Tracker"),
        backgroundColor: Colors.blue,
      ),
      // Shows what the ESP32 is currently sending at the top
      body: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(12.0),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                const Text(
                  'ESP32 Connection',
                  style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                ),
                const SizedBox(height: 8),
                Row(
                  children: [
                    Expanded(
                      child: TextField(
                        controller: _urlController,
                        decoration: const InputDecoration(
                          labelText: 'WebSocket URL',
                          hintText: 'ws://192.168.1.123:81',
                          border: OutlineInputBorder(),
                        ),
                        keyboardType: TextInputType.url,
                      ),
                    ),
                    const SizedBox(width: 10),
                    ElevatedButton(
                      onPressed: _connectToEsp32,
                      child: const Text('Connect'),
                    ),
                  ],
                ),
                const SizedBox(height: 10),
                Row(
                  children: [
                    Icon(
                      _isConnected ? Icons.check_circle : Icons.error,
                      color: _isConnected ? Colors.green : Colors.orange,
                    ),
                    const SizedBox(width: 8),
                    Text('Status: $_connectionStatus'),
                  ],
                ),
                if (_lastError != null) ...[
                  const SizedBox(height: 6),
                  Text(
                    'Error: $_lastError',
                    style: const TextStyle(color: Colors.red),
                  ),
                ],
                const Divider(height: 24),
              ],
            ),
          ),
          Container(
            padding: const EdgeInsets.all(10),
            color: Colors.grey[200],
            child: Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Icon(Icons.sensors, color: Colors.blue),
                const SizedBox(width: 8),
                Text("ESP32: $_esp32Data",
                    style: const TextStyle(fontWeight: FontWeight.bold)),
              ],
            ),
          ),
          Expanded(
            child: ListView.builder(
              itemCount: deliveries.length,
              itemBuilder: (context, index) {
                final item = deliveries[index];

                return Card(
                  margin: EdgeInsets.all(10),
                  child: ListTile(
                    leading: item["delivered"]
                        ? Icon(Icons.check_circle, color: Colors.green)
                        : Image.asset(
                      item["image"],
                      width: 40,
                      height: 40,
                    ),
                    title: Text(item["name"]),
                    subtitle: Text(
                      item["delivered"] ? "Delivered" : "Pending",
                    ),
                    trailing: Text(
                      item["delivered"] ? "Delivered" : "Not Delivered",
                      style: TextStyle(
                        color: item["delivered"] ? Colors.green : Colors.red,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                  ),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}