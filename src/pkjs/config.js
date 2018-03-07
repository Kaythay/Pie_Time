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
        "label": "Set Time Precision To:",
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
        "label": "Set Flower to Change Once Every:",
        "defaultValue": 15,
        "options": [
          { 
            "label": "10 Minutes",
            "value": 10 
          },
          { 
            "label": "15 Minutes",
            "value": 15
          },
          { 
            "label": "30 Minutes",
            "value": 30
          },
          { 
            "label": "1 Hour",
            "value": 0
          }
        ]
      },
      {
        "type": "test",
        "defaultValue": "eg. If 30 minutes is chosen, the flower will change at 0:00, 0:30, 1:00, etc"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];