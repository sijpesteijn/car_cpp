import { Component, OnInit } from '@angular/core';
import {EventService} from "../../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../../camera.service";
import { RaceStripService } from "../../race-strip.service";

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

    updateMetadata(observer: any) {
        if (observer.type === this.type) {
            (this.observer as any).current_pixel_difference = observer.current_pixel_difference;
        }
    }
}