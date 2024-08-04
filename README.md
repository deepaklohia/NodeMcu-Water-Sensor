# Tubewell Water Sensor #

Tubewell Water Sensor is all in one IoT Automation to Detect Tubewell/Motor Running Water and Tank Level as well. It will Ensure that water is coming from tubewell/borewell and turn it off in there is no water for 30seconds (time can be customised). This will save motor from dry run and saves power. We are using this automation for more than 4 years without any issues.

(Basic NodeMCU / IoT knowledge is needed to create this automation)

![Screenshot 2024-07-08 at 9 27 59 PM](https://github.com/deepaklohia/Arduino-and-IoT-Projects/assets/57560249/abae2933-8385-4f38-8d7c-7e4510be5bfc)
![Screenshot 2024-07-08 at 9 28 11 PM](https://github.com/deepaklohia/Arduino-and-IoT-Projects/assets/57560249/1e4be284-cae4-4614-bec1-c8a66e2aa7e2)

### HOW TO USE ###
STEP-1. Connect MCU Board as per circuit Diagram.(Basic knowledge needed)

A.Node MCU Chip https://amzn.to/4eScCE1

B.Soil Sensor for Running Water Check https://amzn.to/3LhP6D2

C.Solid State Relay for pump https://amzn.to/45Uu5re

D.Electronic Relay for Starter https://amzn.to/4co7vK9


<img src="https://github.com/deepaklohia/NodeMcu-Water-Sensor/assets/57560249/facebf75-528f-4b22-996f-6901e90e0e14" width=600 height=300>

STEP-2. Create _Blynk Account_ and Create a Template . Copy your Template ID and Template Name.

STEP-3. Create Virtual Pins on Blynk App using the snapshots .
<!--
![Blynk Virtual Pins](https://github.com/deepaklohia/Arduino-and-IoT-Projects/assets/57560249/e93f260e-4090-49b9-9e20-952cf29f174b)
-->

<img src="https://github.com/deepaklohia/Arduino-and-IoT-Projects/assets/57560249/e93f260e-4090-49b9-9e20-952cf29f174b)" width=600 height=300>

(use help section of Blynk for more info)

STEP-4. Upload the code attached in "Edgent_Tubewell" folder after updating your BLYNK_TEMPLATE_ID/BLYNK_TEMPLATE_NAME,  to Node MCU chip and Connect your device to Blynk app using Wizard on Phone. 

### ORIGINAL AUTOMATION SNAPSHOT ###

![TUBEW](https://github.com/user-attachments/assets/ad601b38-672f-4bce-8742-27acbde5854b)

### MORE INFORMATION ###
How Blynk Edgent Works - its part of blynk it allows to remotely upload the code in the chip . thats a cool feature. 
https://docs.blynk.io/en/blynk.edgent/overview

i am using Maker Plan of Blynk App. you can use free one just remove extra VPIN. i am using it for managing Tenants , so its reimbursed. if you are learner. ,  i would not recommend buying it. 




