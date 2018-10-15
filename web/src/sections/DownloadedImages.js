import React, { Component } from 'react';

export default class MyImages extends Component {
	render() {
		let empty = this.props.gallery.length === 0;
		return (
			<section className={`main-section${this.props.loading ? " loading" : ""}${empty ? " empty":""}`}>
				<div className="gallery">
					{this.props.gallery.map((e, i) => {
						return <div key={i} style={{backgroundImage: "url(/img/"+e.url+")"}}>
							<div>
								<span><i>{e.views}</i> view{e.views !== 1 && "s"} left</span>
							</div>
						</div>;
					})}
				</div>
			</section>
		);
	}
}