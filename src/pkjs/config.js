module.exports = [
  {
    "type": "heading",
    "defaultValue": "Configure"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "select",
        "messageKey": "TimePrecision",
        "label": "Time Precision:",
        "defaultValue": 0,
        "options": [
          { 
            "label": "Seconds",
            "value": 0
          },
          { 
            "label": "Minutes",
            "value": 1
          },
          { 
            "label": "Hours",
            "value": 2
          }
        ]
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "select",
        "messageKey": "FlowerChangePeriod",
        "label": "Flower Change Period:",
        "defaultValue": 10,
        "options": [
          { 
            "label": "10 seconds",
            "value": 10
          },
          { 
            "label": "15 seconds",
            "value": 15
          },
          { 
            "label": "30 seconds",
            "value": 30
          }
        ]
      },
    ]
  },
  {
    "type": "submit",
    "primary": true,
    "defaultValue": "Save Settings"
  }
];