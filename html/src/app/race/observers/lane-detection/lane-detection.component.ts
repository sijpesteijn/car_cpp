import {Component, OnDestroy, ViewChild} from '@angular/core';
import {EventService} from "../../../event.service";
import {AbstractObserverComponent} from "../abstract-observer.component";
import {CameraService} from "../../../camera.service";
import {PreviewComponent} from "../../../preview/preview.component";

@Component({
    selector: 'lane-detection',
    template: require('./lane-detection.html'),
    styles: [require('./lane-detection.scss')]
})
export class LaneDetectionComponent extends AbstractObserverComponent {

    constructor(public cameraService: CameraService,
                public eventService: EventService) {
        super(eventService, cameraService, 'lane_detection');
    }

    updateRoi() {
        if (this.observer) {
            if ((this.observer as any).warp === false && (this.observer as any).previewType === 'warped') {
                (this.observer as any).previewType = 'cvt';
            }
            this.observer.roi.x = 0;
            this.observer.roi.width = this.max_width;
            this.observer.roi.height = this.max_height - this.observer.roi.y;
            this.setRoi();
        }
    }

    updateMetadata(observer: any) {
        if (observer.type === this.type) {
            (this.observer as any).error = observer.error;
            (this.observer as any).angle = observer.angle;
        }
    }

}