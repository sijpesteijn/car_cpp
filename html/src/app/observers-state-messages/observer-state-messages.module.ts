import {NgModule} from "@angular/core";
import {ObserversStateMessagesComponent} from "./observers-state-messages.component";
import {TrafficLightStateComponent} from "./traffic_light_state/traffic-light-state.component";
import {CommonModule} from "@angular/common";
import {LaneDetectionStateComponent} from "./lane_detection_state/lane-detection-state.component";

@NgModule({
    imports: [
        CommonModule
    ],
    exports: [
        ObserversStateMessagesComponent
    ],
    declarations: [
        ObserversStateMessagesComponent,
        LaneDetectionStateComponent,
        TrafficLightStateComponent
    ]
})
export class ObserverStateMessagesModule {}