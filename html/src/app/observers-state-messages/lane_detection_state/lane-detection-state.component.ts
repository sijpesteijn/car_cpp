import {Component, Input} from "@angular/core";
import {ObserverState} from "../../observers_state_messages.service";

@Component({
    selector: 'lane-detection-state',
    template: require('./lane-detection-state.html')
})
export class LaneDetectionStateComponent {
    @Input() state: ObserverState
}