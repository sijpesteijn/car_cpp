import {Component} from '@angular/core';
import {CameraService, CameraSettings} from "../../camera.service";
import {EventService, OBSERVER_ROIS_CLEAR, OBSERVER_ROI_LOADED, OBSERVER_ROI_SET} from "../../event.service";
import {CarObserver} from "../race.service";
import { Config } from "../../app.config";

@Component({
    selector: 'preview-with-rois',
    template: require('./preview-with-rois.html'),
    styles: [require('./preview-with-rois.scss')]
})
export class PreviewWithRoisComponent {
    private cameraSettings: CameraSettings;
    private observers: CarObserver[] = [];
    private streamUrl: string = '/assets/img/testbeeld.png';

    constructor(private cameraService: CameraService,
                private eventService: EventService, private config: Config) {
        this.streamUrl = config.get('stream');
        this.cameraService.getCameraInfo().subscribe(cameraSettings => {
            if (cameraSettings) {
                this.cameraSettings = cameraSettings;
            }
        });
        this.eventService.subscribe(event => {
            if (event.name === OBSERVER_ROI_SET) {
                const newObserver = event.data;
                this.observers = this.observers.filter(observer => observer.type !== newObserver.type);
                this.observers.push(newObserver);
            } else if (event.name === OBSERVER_ROI_LOADED) {
                this.observers.push(event.data);
            } else if (event.name === OBSERVER_ROIS_CLEAR) {
                this.observers = [];
            }
        })
    }
}