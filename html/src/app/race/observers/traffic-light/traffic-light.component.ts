import {Component} from '@angular/core';
import {EventService} from "../../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../../camera.service";

@Component({
    selector: 'traffic-light',
    template: require('./traffic-light.html'),
    styles: [require('./traffic-light.scss')]
})
export class TrafficLightObserverComponent extends AbstractObserverComponent {

    constructor(public cameraService: CameraService,
                public eventService: EventService) {
        super(eventService, cameraService, 'traffic_light');
    }
}