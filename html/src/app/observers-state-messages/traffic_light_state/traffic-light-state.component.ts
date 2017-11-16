import {Component, Input} from "@angular/core";
import {ObserverState} from "../../observers_state_messages.service";

@Component({
    selector: 'traffic-light-state',
    template: require('./traffic-light-state.html')
})
export class TrafficLightStateComponent {
    @Input() state: ObserverState
}