# Arduino-Stepper-LCD-Shield
<p>Set Up Control Parameter For Stepper Movement using LCD shield</p>

# Connection

### LCD Shield
<table style="width:100%">
  <tr>
    <th>LCD</th>
    <th>Arduino</th>
    <th>Keys</th>
    <th>Arduino</th>
  </tr>
  <tr>
    <td>RS</td>
    <td>D8</td>
    <td>UP</td>
    <td>A0</td>
  </tr>
  <tr>
    <td>E</td>
    <td>D9</td>
    <td>DOWN</td>
    <td>A0</td>
  </tr>
  <tr>
    <td>D4</td>
    <td>D4</td>
    <td>RIGHT</td>
    <td>A0</td>
  </tr>
  <tr>
    <td>D5</td>
    <td>D5</td>
    <td>LEFT</td>
    <td>A0</td>
  </tr>
  <tr>
    <td>D6</td>
    <td>D6</td>
    <td>Select</td>
    <td>A0</td>
  </tr>
  <tr>
    <td>D7</td>
    <td>D7</td>
  </tr>
</table>

### Stepper Motor Driver
<table style="width:100%">
  <tr>
    <th>Motor Driver</th>
    <th>Arduino</th>
  </tr>
  <tr>
    <td>Pulse+</td>
    <td>48</td>
  </tr>
  <tr>
    <td>Pulse-</td>
    <td>GND</td>    
  </tr>
  <tr>
    <td>Direction+</td>
    <td>50</td>
  </tr>
  <tr>
    <td>Direction-</td>
    <td>GND</td>
  </tr>
  <tr>
    <td>Enable+</td>
    <td>52</td>
  </tr>
  <tr>
    <td>Enable-</td>
    <td>GND</td>
  </tr>
</table>

## Hardware
<p>1 x Arduino Mega2560 </p>
<p>1 x 16x2 (1602) LCD Keypad Shield</p>
<p>1 x TB6600 Stepper Motor Driver. Can use other stepper motor driver, for example DRV8834, DRV8825, DRV8824, A4988.</p>
<p>1 x NEMA 23 10.1 kg-cm Hybrid Stepper Motor</p>
<p>1 x 12v 5A SMPS</p>
