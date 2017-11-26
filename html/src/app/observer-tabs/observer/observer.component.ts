import {Component, Input} from "@angular/core";
import {CarObserver} from "../../observers/observer.service";

@Component({
    selector: 'observer',
    template: require('./observer.html')
})
export class ObserverComponent {
    @Input() observer: CarObserver;
}