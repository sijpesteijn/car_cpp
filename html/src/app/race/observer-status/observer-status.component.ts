import {Component, Input} from "@angular/core";

@Component({
    selector: 'observer-status',
    template: require('./observer-status.html'),
    styles: [require('./observer-status.scss')]
})
export class ObserverStatusComponent {
    @Input() selected: boolean;
    @Input() achieved: boolean;
}