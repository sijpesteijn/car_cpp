import {Component} from '@angular/core';
import {CarObserver, ObserverService } from '../observer.service';
import {EventService} from "../../event.service";
import {ObserversStateMessagesService, ObserverState} from "../../observers_state_messages.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../camera.service";

interface TrafficLightObserver extends CarObserver {
    current_pixel_difference: number;
}

interface TrafficLightObserverState extends ObserverState {
    current_pixel_difference: number;
    condition_achieved: number;
}
@Component({
    selector: 'traffic-light',
    template: require('./traffic-light.html'),
    styles: [require('./traffic-light.scss')]
})
export class TrafficLightObserverComponent extends AbstractObserverComponent {

    constructor(public observerService: ObserverService,
                public observersStateMessagesService: ObserversStateMessagesService,
                public cameraService: CameraService,
                public eventService: EventService) {
        super(observerService, eventService, cameraService, 'traffic_light');
    }

    ngAfterViewInit() {
        // this.loadObserver();
        this.observersStateMessagesService.getLog().subscribe(states => {
            if (this.observer && states && states.length > 0) {
                const state = states.filter(state => state.type === this.type)
                if (state && state.length === 1) {
                    this.observer.condition_achieved = (state[0] as TrafficLightObserverState).condition_achieved;
                    (this.observer as TrafficLightObserver).current_pixel_difference = (state[0] as TrafficLightObserverState).current_pixel_difference;
                }
            }
        });
    }
}