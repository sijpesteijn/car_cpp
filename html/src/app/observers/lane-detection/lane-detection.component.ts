import {Component} from '@angular/core';
import { ObserverService } from '../observer.service';
import {EventService} from "../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../camera.service";

@Component({
    selector: 'lane-detection',
    template: require('./lane-detection.html'),
    styles: [require('./lane-detection.scss')]
})
export class LaneDetectionComponent extends AbstractObserverComponent {

    constructor(public observerService: ObserverService,
                public cameraService: CameraService,
                public eventService: EventService) {
        super(observerService, eventService, cameraService, 'lane_detection');
    }

    updateRoi() {
        this.observer.roi.height = this.max_height - this.observer.roi.y;
        this.setRoi();
    }

    updateThreshold1(event: any) {
        (this.observer as any).threshold1 = parseFloat(event.target.value);
        this.updateRoi();
    }

    updateThreshold2(event: any) {
        (this.observer as any).threshold2 = parseFloat(event.target.value);
        this.updateRoi();
    }
}