import {Component} from '@angular/core';
import { ObserverService } from '../observer.service';
import {EventService} from "../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../camera.service";

@Component({
    selector: 'finish-detection',
    template: require('./finish-detection.html'),
    styles: [require('./finish-detection.scss')]
})
export class FinishDetectionComponent extends AbstractObserverComponent {

    constructor(public observerService: ObserverService,
                public cameraService: CameraService,
                public eventService: EventService) {
        super(observerService, eventService, cameraService, 'finish_detection')
    }

    updateRoi() {
        this.observer.roi.height = this.max_height - this.observer.roi.y;
        this.setRoi();
    }
}