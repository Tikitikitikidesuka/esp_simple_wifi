<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h3 align="center">Simple ESP Wifi Library</h3>

  <p align="center">
    A hassle-free way of adding wifi to your ESP-IDF projects!
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>


<!-- ABOUT THE PROJECT -->
## About The Project

The ESP-IDF Wi-Fi library can be challenging to use, as it demands an understanding of the underlying FreeRTOS system and often involves a significant amount of boilerplate code. This library simplifies the process, enabling anyone to quickly add Wi-Fi functionality to any esp project. This eliminates the need to navigate the sometimes unnecessary complexity of the 'esp_wifi.h' library.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

This library can only be used in ESP-IDF projects and, therefore, requires that the ESP-IDF development environment be installed. Instructions on how to install it can be found in [Espressif's website](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).

### Installation

To add the library to a project, simply copy the `esp_ez_wifi_sta` directory to the `components` directory of your ESP-IDF project. If the `components` directory does not exist, create it next to the `main` directory.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- USAGE EXAMPLES -->
## Usage

An example can be found in the `main` directory of this repository. It connects to a wifi given it's SSID and password and prints the ip assigned.

```c
#include "esp_ez_wifi_sta.h"

#include "freertos/FreeRTOS.h"

void app_main() {
  sta_start();

  sta_connect("ssid", "password");

  esp_netif_ip_info_t connection_info;
  if (sta_connection_info(&connection_info)) {
    printf("ip: " IPSTR "\n", IP2STR(&connection_info.ip));
  }
}


```

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTACT -->
## Contact

Tikitikitikidesuka - deesneakygerbil@gmail.com

Project Link: [https://github.com/Tikitikitikidesuka/esp_ez_wifi_sta](https://github.com/Tikitikitikidesuka/esp_ez_wifi_sta)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
