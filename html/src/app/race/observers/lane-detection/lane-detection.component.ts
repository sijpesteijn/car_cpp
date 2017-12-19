import {Component} from '@angular/core';
import {EventService} from "../../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../../camera.service";

@Component({
    selector: 'lane-detection',
    template: require('./lane-detection.html'),
    styles: [require('./lane-detection.scss')]
})
export class LaneDetectionComponent extends AbstractObserverComponent {
    private filter: string = 'cvt';

    constructor(public cameraService: CameraService,
                public eventService: EventService) {
        super(eventService, cameraService, 'lane_detection');
    }

    updateRoi() {
        if (this.observer) {
            this.observer.roi.x = 0;
            this.observer.roi.width = this.max_width;
            this.observer.roi.height = this.max_height - this.observer.roi.y;
            this.setRoi();
        }
    }
}